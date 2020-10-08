FROM python:3.7-alpine
LABEL maintainer="PGGP"
ENV FLASK_RUN_HOST=0.0.0.0
WORKDIR /code
COPY requirements.txt requirements.txt
RUN pip install -r requirements.txt
COPY . .
#CMD ["python", "-u", "app.py"]
CMD ["flask", "run", "-h 0.0.0.0", "-p 80"]
EXPOSE 80