FROM debian 
LABEL maintainer="PGGP" 
RUN apt-get update 
RUN apt-get install -y python3-venv python3-pip
RUN apt-get install -y git 
RUN pip3 install Flask 
ARG username
ARG password
RUN git clone https://$username:$password@github.com/pgperusina/proyecto1so1.git
WORKDIR "/proyecto1so1/server1/src" 
RUN pwd
RUN git pull
RUN pwd
RUN ls -lh
RUN python3 router.py
CMD ["/bin/bash"]
EXPOSE 80