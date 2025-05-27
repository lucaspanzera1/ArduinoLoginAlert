import fetch from 'node-fetch';
import { SerialPort } from 'serialport';

const port = new SerialPort({ path: '/dev/ttyACM0', baudRate: 9600 });

port.on('open', () => {
  console.log('Porta serial aberta!');
});

port.on('error', (err) => {
  console.error('Erro na porta serial:', err.message);
});

setInterval(async () => {
  try {
    const response = await fetch('https://taskmanagerapi-production-ad12.up.railway.app/api/arduino/comando');
    const data = await response.json();

    if (data.comando) {
      console.log('Comando novo recebido:', data.comando);

      port.write(`${data.comando}\n`, (err) => {
        if (err) return console.error('Erro ao enviar comando:', err.message);
        console.log(`Comando "${data.comando}" enviado ao Arduino.`);
      });
    }
  } catch (error) {
    console.error('Erro ao buscar comando:', error.message);
  }
}, 3000); // verifica a cada 3 segundos
