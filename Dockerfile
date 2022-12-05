#node-red
FROM nodered/node-red:latest

# install telegram
RUN npm install node-red-contrib-telegrambot

# Start node-red
CMD ["npm", "start", "--", "--userDir", "/data"]