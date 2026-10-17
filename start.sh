#!/bin/bash

streamlit run frontend/app.py \
    --server.port=8003 \
    --server.baseUrlPath=/peptideranker \
    --server.enableCORS=false \
    --server.enableXsrfProtection=false &

uvicorn be_app:app \
    --host 0.0.0.0 --port 8501

wait
