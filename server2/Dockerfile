FROM debian 
LABEL maintainer="PGGP" 
RUN apt-get -y update 
RUN apt-get install -y apache2
RUN apt-get install -y git
RUN apt-get install -y npm
RUN apt-get install -y nodejs

WORKDIR "/code"
COPY . .
CMD ["/bin/bash"]
EXPOSE 80