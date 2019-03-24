'use strict';

console.log('Loading function');
var AWS = require("aws-sdk");
var highest;
var Lowest;
//Readfrom DB
AWS.config.update({region: 'us-east-1'});
var ddb = new AWS.DynamoDB({apiVersion: '2012-08-10'});
//Readfrom DB

exports.handler = (event, context, callback) => {
    
//WriteDB-Begin
var paramsWriteto = {
  TableName: 'GreenhouseDatabase',
  Item: {
    'reported' : {S: 'test1'},
    'index' : {S: '12314'},
    'NumberDa': {N: '111'}
  }
};
ddb.putItem(paramsWriteto, function(err, data) {
  if (err) {
    console.log("Error Putting", err);
  } else {
    console.log("Success Putting to DynamoDB");
  }
});
//WriteDB-End

//ReadDB-Begin
var paramsReadfrom = {
  TableName: 'GreenhouseDatabase',
  Key: {
    'reported' : {S: 'Temperature_range'},
    'index' : {S: '1111'},
  }
};
ddb.getItem(paramsReadfrom, function(err, data) {
  if (err) {
    console.log("Error Reading",err);
  } else {
    
    console.log("Success Reading from DynamoDB");
  }
  var Jsondata = JSON.stringify(data, null, 2);
  var Highest_temperature= JSON.parse(Jsondata).Item.Highest_temperature.N;
  var Lowest_temperature = JSON.parse(Jsondata).Item.Lowest_temperature.N;
  
  //SendMessage-Begin
  var datapackage = JSON.stringify(event, null, 2);
  console.log('Data Receved from IOT');
  var Date =  JSON.parse(datapackage).reported.Date;
  var Time =  JSON.parse(datapackage).reported.Time;
  var Temperature_inside =  JSON.parse(datapackage).reported.Temperature_inside;
  var Temperature_outside =  JSON.parse(datapackage).reported.Temperature_outside;
  var Humidmity =  JSON.parse(datapackage).reported.Humidity;
  var message1 = 'Current Time: ' + Date + '-' + Time +'\n';
  var message2 = 'The Temperature inside the greenhouse is: ' + Temperature_inside + 'degrees.\n';
  var message3 = 'The Temperature outside the greenhouse is: ' + Temperature_outside + 'degrees.\n';
  var message4 = 'The Humidity inside the greenhouse is ' + Humidmity + '%' + '\n';
  var message5 = 'Highest Temperature is ' + Highest_temperature + '\n';
  var message6 = 'Lowest Temperature is ' + Lowest_temperature + '\n';
  var messageupdate = message1 + message2 + message3 + message4 + message5 + message6;
  
  var messagewarning = 'Temperature is ' + Temperature_inside + ' degrees, too low.\n';
  var messagesent;
  if (Temperature_inside < Lowest_temperature) {
    messagesent = messagewarning;
  }
  else {
    messagesent = messageupdate;
  }
  var sns = new AWS.SNS();
  var params = {
    Message: messagesent,
    TopicArn: "arn:aws:sns:us-east-1:812365191913:GreenhouseTemAlert"
  };
  sns.publish(params, context.done);
  //SendMessage-END
});

console.log('The highest temperature is  ' + highest);
console.log('The lowest temperature is  ' + Lowest);
//ReadDB-End



};