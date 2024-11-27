FROM nodered/node-red:latest

WORKDIR /data

COPY flows.json flows.json

RUN npm install node-red-dashboard

WORKDIR /usr/src/node-red