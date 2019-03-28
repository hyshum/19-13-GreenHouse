
const request = require('request');


exports.handler = (event, context, callback) => {
    Switchon_off();          
};

function Switchon_off() {
    console.log('Running Switch');
    const option = 'https://maker.ifttt.com/trigger/ec464greenhouseOFF/with/key/cvJYmevJ910Cxw7Zr5Y6Ac';
    request(option, function (error, response, body) {
        console.log('error:', error); // Print the error if one occurred
        console.log('statusCode:', response && response.statusCode); // Print the response status code if a response was received
        console.log('body:', body); // Print the HTML for the Google homepage.
      });
}
