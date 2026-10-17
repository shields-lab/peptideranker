import copy
import datetime as dt
import os
import os.path as osp
import subprocess
import time

from queue import Queue
from threading import Thread

import psutil
import pandas as pd
from fastapi import FastAPI
from pydantic import BaseModel

from utils import send_mail


# -----------------------
# Paths
# -----------------------
BASE_DIR = osp.dirname(__file__)

JOBS = osp.join(BASE_DIR, "backend")
os.makedirs(JOBS, exist_ok=True)

os.environ["TOKENIZERS_PARALLELISM"] = "false"


# -----------------------
# Email templates
# -----------------------
SUCCESS_MSG = """
Your PeptideRanker job has finished successfully!

This job was submitted on {time}.

Sincerely,
The PeptideRanker team
"""

ERROR_MSG = """
Your PeptideRanker job has failed.

This job was submitted on {time}.

Error:
{error}

Sincerely,
The PeptideRanker team
"""


# -----------------------
# FastAPI app
# -----------------------
app = FastAPI()


# -----------------------
# Request model
# -----------------------
class Job(BaseModel):
    email: str
    text: str
    job_id: str


# -----------------------
# Globals
# -----------------------
job_queue = Queue()
job_status = {}

CLEANUP_AFTER_DAYS = 2


# -----------------------
# Cleanup
# -----------------------
def cleanup_jobs():
    now = dt.datetime.now()
    to_delete = []

    for job_id, data in job_status.items():
        job_time = data.get("time")

        if isinstance(job_time, dt.datetime):
            if data.get("status") in {"completed", "failed"}:
                if now - job_time > dt.timedelta(days=CLEANUP_AFTER_DAYS):
                    to_delete.append(job_id)

    for job_id in to_delete:
        del job_status[job_id]


def cleanup_worker():
    while True:
        time.sleep(3600)  # run every hour
        cleanup_jobs()


# -----------------------
# Worker
# -----------------------
def worker():
    while True:
        job = job_queue.get()
        if job is None:
            break

        job_id = job.job_id

        try:
            filename = osp.abspath(osp.join(JOBS, f"{job_id}.txt"))
            outname = osp.abspath(osp.join(JOBS, f"{job_id}.tsv"))

            with open(filename, "w") as f:
                f.write(job.text)

            cwd = os.getcwd()
            os.chdir(JOBS)

            p = subprocess.Popen(["./PeptideRanker.pl", filename, outname])

            proc = psutil.Process(p.pid)
            proc.cpu_affinity([i for i in range(5)])

            p.wait()

            df = pd.read_csv(outname, sep="\t", header=None,
                              names=["prediction", "sequence"])

            df = df[["sequence", "prediction"]]
            df.to_csv(outname, sep="\t", index=False)

            os.chdir(cwd)

            now = dt.datetime.now()

            job_status[job_id] = {
                "status": "completed",
                "email": job.email,
                "result": outname,
                "time": now
            }

            send_mail(
                send_to=[job.email, "autopeptideml@gmail.com"],
                subject=f"PeptideRanker Results: {job_id}",
                text=SUCCESS_MSG.format(time=now.strftime("%m/%d/%Y, %H:%M:%S")),
                username="autopeptideml",
                app_passwd="nvgq pktc ycwm unnm",
                files=[outname]
            )

            os.remove(filename)
            os.remove(outname)

        except Exception as e:
            now = dt.datetime.now()

            job_status[job_id] = {
                "status": "failed",
                "email": job.email,
                "error": str(e),
                "time": now
            }

            send_mail(
                send_to=[job.email, f"{os.getenv('EMAIL_USERNAME')}@gmail.com"],
                subject=f"PeptideRanker Failed: {job_id}",
                text=ERROR_MSG.format(
                    time=now.strftime("%m/%d/%Y, %H:%M:%S"),
                    error=str(e)
                ),
                username=os.getenv("EMAIL_USERNAME"),
                app_passwd=os.getenv("EMAIL_APP_PASSWORD")
            )

        job_queue.task_done()


# -----------------------
# Start workers
# -----------------------
NUM_WORKERS = 2

for _ in range(NUM_WORKERS):
    Thread(target=worker, daemon=True).start()

Thread(target=cleanup_worker, daemon=True).start()


# -----------------------
# API endpoints
# -----------------------
@app.post("/submit-job/")
def submit_job(job: Job):

    job_status[job.job_id] = {
        "status": "queued",
        "email": job.email,
        "id": job.job_id,
        "time": dt.datetime.now()
    }

    job_queue.put(job)

    return {"message": "Job submitted", "job_id": job.job_id}


@app.get("/job-status/{job_id}")
def get_job_status(job_id: str):
    return job_status.get(job_id, {"error": "Job not found"})


@app.get("/queue/")
def get_queue():
    return copy.deepcopy(job_status)