FROM debian 
LABEL maintainer="PGGP" 
RUN apt-get -y update 
RUN apt-get install -y git
RUN apt-get install -y npm
RUN apt-get install -y nodejs

WORKDIR "/code"
COPY . .
WORKDIR "/code/app/client"
RUN npm install
RUN npm start
CMD ["sleep", "infinity"]
EXPOSE 80