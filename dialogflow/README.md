# Astro Remote Home Dialogflow Integration

Dialogflow is a Google-owned developer of human–computer interaction technologies based on natural language conversations.
With this integration you can control AstroRemoteHome via the Goolge Assistant and other chatbot interfaces already established by dialogflow.

# Installation
1. Create a dialogflow account and upload all files from the settings directory as zip file.

2. Start a Python app with the program code from the fullfillment directory. The app reads all necessary variables from the environment, and can be easily served on services like Heroku.

<center>
key                 |  value
:------------------:|:-----------------------------:|
BASIC_AUTH_PASSWORD |  ???
BASIC_AUTH_USERNAME |  ???
CLOUDMQTT_URL       |  mqtt://user:pass@server:port
</center>
    
3. Register the webhook url as dialogflow fullfilment with the basic auth config from above.