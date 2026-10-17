FROM python:3.12-slim-bookworm

WORKDIR /app
ENV LOCAL_USR=$USER

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    wget \
    gnupg \
    ca-certificates \
    dirmngr \
    build-essential \
    python3-dev \
    gfortran \
    libcurl4-openssl-dev \
    libssl-dev \
    libxml2-dev \
    r-base && \
    \
    python3 -m pip install --upgrade pip wheel && \
    python3 -m pip install --no-cache-dir \
    streamlit \
    configparser \
    pandas \
    scikit-learn \
    matplotlib \
    scipy \
    psutil \
    fastapi \
    numpy && \
    \
    apt-get purge -y build-essential python3-dev gfortran && \
    apt-get autoremove -y && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

COPY . /app

WORKDIR /app/
EXPOSE 8003 8501

COPY start.sh /start.sh
USER ${LOCAL_USR}
RUN chmod +x /start.sh

CMD ["/start.sh"]
