import React, {useEffect} from 'react';
import Navigator from './routes/homeStack';
//import MQTTConnection from './MQTTConnection';

export default function App() {
  
  /*useEffect(() => {
    this.mqttConnect = new MQTTConnection();
    this.mqttConnect.onMQTTConnect = this.onMQTTConnect;
    this.mqttConnect.onMQTTLost = this.onMQTTLost;
    this.mqttConnect.onMQTTMessageArrived = this.onMQTTMessageArrived;
    this.mqttConnect.onMQTTMessageDelivered = this.onMQTTMessageDelivered;

    this.mqttConnect.connect('169.254.227.218', 1883);

    onMQTTConnect = () => {
        console.log('App onMQTTConnect');
        this.mqttConnect.subscribeChannel('dev/test');
    }

    onMQTTLost = () => {
        console.log('App onMQTTLost');
    }

    onMQTTMessageArrived = (message) => {
        console.log('App onMQTTMessageArrived: ', message);
        console.log('App onMQTTMessageArrived payloadString: ', message.payloadString);
    }

    onMQTTMessageDelivered = (message) => {
        console.log('App onMQTTMessageDelivered: ', message);
    }
    return () => {
      this.mqttConnect.close();
    }

  }, [])*/
  return (
    <Navigator />
  );
}
