import React from 'react';
import { StyleSheet } from 'react-native';
//import rows from './server.js'; //server.js pass to App.js
import { globalstyles } from './styles/globalstyles';
import Navigator from './routes/homeStack';


export default function App() {

  return (
    <Navigator />
  );
}
//const rowz = rows;