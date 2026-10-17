import os
import os.path as osp
import time
import subprocess
import requests
import json
import pandas as pd
import streamlit as st


API_BASE = "http://localhost:8501"

st.set_page_config(
    page_title="PeptideRanker",
    layout="centered",
    menu_items={"About": "http://peptide.ucd.ie/shieldslab"},
)

AMINO_ACIDS = [
    "A", "C", "D", "E", "F", "G",
    "H", "I", "K", "L", "M", "N",
    "P", "Q", "R", "S", "T", "V",
    "W", "Y", "X"
]
MAX_PEPTIDES = 1_000


def input_validation(text: str) -> str:
    if len(text) > MAX_PEPTIDES:
        mssg = f"The webserver is limited to running {MAX_PEPTIDES} peptides at a given time. "
        mssg += f"Your query has {len(text):,}. Please breakdown your query into smaller chunks."
        st.error(mssg)
    for idx, peptide in enumerate(text):
        for c in peptide:
            if c not in AMINO_ACIDS:
                mssg = f"Amino acid: ``{c}`` in Peptide {idx} - ``{peptide}`` is not supported."
                mssg += " Please clean up the data so that it only contains canonical amino acids: "
                mssg += ", ".join(AMINO_ACIDS)
                st.error(mssg)
    return text


def home():
    st.title("PeptideRanker")
    if 'job-submitted' in st.session_state:
        st.success(
            """
            Your job has been sucessfully launched!
            The results will shortly be sent to the email provided.
            The job may take from a few minutes to a few hours,
            depending on the size of the dataset.
            """
        )
        st.stop()
    elif "status" not in st.session_state:
        with st.sidebar:
            st.markdown(
                """
                ## Description

                Peptide ranker is a server for the prediction of bioactive peptides
                based on a novel N-to-1 neural network. Users may submit a list of
                peptides to PeptideRanker which will be returned to the user ranked by the
                probability that the peptide will be bioactive. It is important
                to note that this is not a prediction of the degree of bioactivity.

                ## Pages
                - Paper: [PlosOne](https://journals.plos.org/plosone/article?id=10.1371/journal.pone.0045012)
                - Training Datasets available upon request to:
                    - [Catherine Mooney](mailto:catherine.mooney@ucd.ie)
                    - [Denis Shields](mailto:denis.shields@ucd.ie)
                """
            )

        st.markdown(
            """
            ### Enter peptide amino acid one-letter sequences

            One peptide per line. Max length 150 residues per peptide.

            If a peptide is broken across multiple lines, each line will be treated
            as a separate peptide.
            """
        )

        text = st.text_area(
            "Input peptide sequences",
            placeholder="AATC\nMTERRRASDDDA"
        )
        text = input_validation(text.split("\n"))
        email = st.text_input(
            "Introduce the email to which the results will be sent",
            type='default',
        )
        if len(email.split("@")) != 2 or '.' not in email:
            st.warning("Please input a proper email.")
        button = st.button("🚀 Run")
        if button:
            st.session_state['status'] = "run"
            st.session_state["text"] = "\n".join(text)
            st.session_state["email"] = email
            st.rerun()
    else:
        st.markdown(
            """
            ### PeptideRanker Results

            PeptideRanker orders the peptides you entered according to their score.
            """
        )
        data = {
            'email': st.session_state['email'],
            'text': st.session_state['text'],
            "job_id": str(time.time()) + str(hash(st.session_state['email']))
        }
        response = requests.post(
            f"{API_BASE}/submit-job/",
            data=json.dumps(data),
            headers={"Content-Type": "application/json"}
        )
        if response is None:
            st.stop()
        elif response.status_code == 200:
            st.session_state['job-submitted'] = True
            st.rerun()


if __name__ == "__main__":
    home()
