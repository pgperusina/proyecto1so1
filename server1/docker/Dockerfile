FROM debian 
LABEL maintainer="PGGP" 
RUN apt-get update 
RUN apt-get install -y python3-venv python3-pip
RUN apt-get install -y git 
RUN pip3 install Flask 
ARG username
ARG password
WORKDIR "/home"
RUN git clone https://$username:$password@github.com/pgperusina/proyecto1so1.git
WORKDIR "/home/proyecto1so1/server1/src" 
RUN git checkout develop
RUN git pull
CMD python3 router.py
EXPOSE 80