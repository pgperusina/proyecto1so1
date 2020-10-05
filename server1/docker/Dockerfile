FROM debian 
LABEL maintainer="PGGP" 
RUN apt-get update 
RUN apt-get install python3-venv python3-pip
RUN apt-get install -y git 
RUN pip3 install Flask 
RUN git clone git://github.com:pgperusina/proyecto1so1.git 
WORKDIR "/proyecto1so1/server1/src" 
RUN git pull CMD ["python3 router.py", "-D" "FOREGROUND"]
EXPOSE 80