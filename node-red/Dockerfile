#node-red
FROM nodered/node-red:latest

# install telegram
RUN npm install node-red-contrib-telegrambot

# copy initial flow file
COPY flows.json /data/flows.json

# Start node-red
CMD ["npm", "start", "--", "--userDir", "/data"]
