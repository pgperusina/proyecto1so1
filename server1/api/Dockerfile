# FROM debian 
# LABEL maintainer="PGGP" 
# RUN apt-get -y update 
# RUN apt-get install -y python3-venv python3-pip
# RUN apt-get install -y git 
# RUN pip3 install Flask 
# ARG username
# ARG password
# WORKDIR "/home"
# RUN git clone https://$username:$password@github.com/pgperusina/proyecto1so1.git
# WORKDIR "/home/proyecto1so1/server1/src" 
# RUN git checkout develop
# RUN git pull
# CMD ["flask", "run", "-h 0.0.0.0", "-p 80"]
# EXPOSE 80

FROM python:3.7-alpine
LABEL maintainer="PGGP"
ENV FLASK_RUN_HOST=0.0.0.0
WORKDIR /code
COPY requirements.txt requirements.txt
RUN pip install -r requirements.txt
COPY . .
CMD ["flask", "run"]
EXPOSE 80