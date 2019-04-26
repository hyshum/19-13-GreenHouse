//Slow the data


'use strict';

console.log('Loading function');
var AWS = require("aws-sdk");
//const request = require('request');
//Readfrom DB
AWS.config.update({ region: 'us-east-1' });
var ddb = new AWS.DynamoDB({ apiVersion: '2012-08-10' });
//Readfrom DB

exports.handler = (event, context, callback) => {
    var today = new Date();
    var Year = today.getFullYear().toString();
    var Month = today.getMonth();
    var Day = today.getDate();
    var Hour = today.getHours() - 4;
    if (Hour < 0) {
        Hour += 24;
    }
    var Minute = today.getMinutes();
    var Second = today.getSeconds();
    if (Month.toString().length == 1) {
        Month = '0' + (Month + 1).toString();
    }
    if (Day.toString().length == 1) {
        Day = '0' + (Day).toString();
    }
    if (Hour.toString().length == 1) {
        Hour = '0' + (Hour).toString();
    }
    if (Minute.toString().length == 1) {
        Minute = '0' + (Minute).toString();
    }
    if (Second.toString().length == 1) {
        Second = '0' + (Second).toString();
    }
    var YMD = Year.toString() + Month.toString() + Day.toString();
    var HMS = Hour.toString() + Minute.toString() + Second.toString();
    Read_IOT_Data(event, YMD, HMS, function (IOT_data) {
        if (IOT_data[9] == 1) {
            if (IOT_data[2] == 0 && IOT_data[3] == 0 && IOT_data[4] == 0 && IOT_data[5] == 0 && IOT_data[6] == 0 && IOT_data[7] == 0 && IOT_data[8] == 0) {
                console.log('Data package is empty');
            }
            else {
                Read_Interval(function (Interval) {
                    console.log(Interval);
                    if (Interval > 20) {
                        Write_Interval(0);
                        Read_Lowest_temperature(function (Lowest_temperature) {
                            Read_Highest_temperature(function (Highest_temperature) {
                                Read_Specific_time(function (Specific_time) {
                                    Decide_month(IOT_data, function (Month) {
                                        Read_Last_time(function (Last_time) {
                                            Read_Heater_runningtime(Month, function (Heater_runningtime) {
                                                Read_Heater_current(function (Heater_current) {
                                                    Write_Item_dynamoDB(IOT_data);
                                                    Write_Current_time(IOT_data[1]);
                                                    Switching_On_OFF(IOT_data, Lowest_temperature, Highest_temperature, Heater_current);
                                                    Count_Heater(Month, Heater_current, parseInt(Last_time, 10), IOT_data, parseInt(Heater_runningtime, 10), function (Heater_runningtime_total) {
                                                        Send_Update_Message(IOT_data, context, Last_time, Specific_time, Heater_runningtime_total, Month);
                                                    });
                                                });
                                            });
                                        });
                                    });
                                });
                            });
                        });
                    }
                    else {
                        Write_Interval((Interval * 1) + 1);
                    }
                })
            }
        }
    });
}
function Read_Lowest_temperature(callback) {
    var params_Lowest_temperature = {
        TableName: 'GreenhouseDatabase',
        Key: {
            'reported': { S: 'Lowest_temperature' },
            'index': { S: '0002' },
        }
    };
    ddb.getItem(params_Lowest_temperature, function (err, data, response) {
        if (err) {
            console.log("Error Reading Lowest_temperature", err);
        }
        else {
            console.log("Success Reading Lowest_temperature");
            var Jsondata = JSON.stringify(data, null, 2);
            var Lowest_temperature = JSON.parse(Jsondata).Item.val.S;
            console.log(Lowest_temperature);
            return callback(Lowest_temperature);
        }
    });
}

function Read_Highest_temperature(callback) {
    var params_Highest_temperature = {
        TableName: 'GreenhouseDatabase',
        Key: {
            'reported': { S: 'Highest_temperature' },
            'index': { S: '0002' },
        }
    };
    ddb.getItem(params_Highest_temperature, function (err, data, response) {
        if (err) {
            console.log("Error Reading Highest_temperature", err);
        }
        else {
            console.log("Success Reading Highest_temperature");
            var Jsondata = JSON.stringify(data, null, 2);
            var Highest_temperature = JSON.parse(Jsondata).Item.val.S;
            return callback(Highest_temperature);
        }
    });
}

function Read_Specific_time(callback) {
    var params_Interval = {
        TableName: 'GreenhouseDatabase',
        Key: {
            'reported': { S: 'Specific_time' },
            'index': { S: '0002' },
        }
    };
    ddb.getItem(params_Interval, function (err, data, response) {
        if (err) {
            console.log("Error Reading Specific_time", err);
        }
        else {
            console.log("Success Reading Specific_time");
            var Jsondata = JSON.stringify(data, null, 2);
            var Specific_time = JSON.parse(Jsondata).Item.val.S;
            Specific_time = Specific_time + '00';
            return callback(Specific_time);
        }
    });
}

function Read_Last_time(callback) {
    var params_Last_time = {
        TableName: 'GreenhouseDatabase',
        Key: {
            'reported': { S: 'Last_time' },
            'index': { S: '0001' },
        }
    };
    ddb.getItem(params_Last_time, function (err, data, response) {
        if (err) {
            console.log("Error Reading Last_time", err);
        }
        else {
            console.log("Success Reading Last_time");
            var Jsondata = JSON.stringify(data, null, 2);
            var Last_time = JSON.parse(Jsondata).Item.val.N;
            return callback(Last_time);
        }
    });
}

function Read_Heater_current(callback) {
    var params_Last_time = {
        TableName: 'GreenhouseDatabase',
        Key: {
            'reported': { S: 'Heater_current' },
            'index': { S: '0001' },
        }
    };
    ddb.getItem(params_Last_time, function (err, data, response) {
        if (err) {
            console.log("Error Reading Heater_current", err);
        }
        else {
            console.log("Success Reading Heater_current");
            var Jsondata = JSON.stringify(data, null, 2);
            var Heater_current = JSON.parse(Jsondata).Item.val.N;
            return callback(Heater_current);
        }
    });
}

function Read_IOT_Data(event, YMD, HMS, callback) {
    var datapackage = JSON.stringify(event, null, 2);
    console.log('Data Receved from IOT');
    //console.log(datapackage);
    var IOT_data_string = JSON.parse(datapackage).reported.t;
    var IOT_data = IOT_data_string.split(';');
    var Temperature_inside = IOT_data[0];
    var Temperature_outside = IOT_data[1];
    var Humidmity = IOT_data[2];
    var Light_level = IOT_data[3];
    var Soil_Moisture_1 = IOT_data[4];
    var Soil_Moisture_2 = IOT_data[5];
    var Soil_Moisture_3 = IOT_data[6];
    var index = IOT_data[7];
    var IOT_data = [YMD, HMS, Temperature_inside, Temperature_outside, Humidmity, Light_level, Soil_Moisture_1, Soil_Moisture_2, Soil_Moisture_3, index];
    return callback(IOT_data);
}


function Write_Item_dynamoDB(IOT_data) {
    var params_IOT_data = {
        TableName: 'GreenhouseDatabase',
        Item: {
            'reported': { S: 'Sensor_Data' },
            'index': { S: IOT_data[0].toString() + IOT_data[1].toString() },
            'Date': { N: IOT_data[0].toString() },
            'Time': { N: IOT_data[1].toString() },
            'Temperature_inside': { N: IOT_data[2].toString() },
            'Temperature_outside': { N: IOT_data[3].toString() },
            'Humidmity': { N: IOT_data[4].toString() },
            'Light_level': { N: IOT_data[5].toString() },
            'Soil_Moisture_1': { N: IOT_data[6].toString() },
            'Soil_Moisture_2': { N: IOT_data[7].toString() },
            'Soil_Moisture_3': { N: IOT_data[8].toString() }
        }
    };
    ddb.putItem(params_IOT_data, function (err, data) {
        if (err) {
            console.log("Error Creating Item", err);
        } else {
            console.log("Success Creating Item");
        }
    });
}


function Write_Current_time(Current_time_input) {
    var params_Current_time = {
        TableName: 'GreenhouseDatabase',
        Item: {
            'reported': { S: 'Last_time' },
            'index': { S: '0001' },
            'val': { N: Current_time_input.toString() }
        }
    };
    ddb.putItem(params_Current_time, function (err, data) {
        if (err) {
            console.log("Error Writing Current_time", err);
        } else {
            console.log("Success Writing Current_time");
        }
    });
}

function Write_Heater_current(Heater_current) {
    var params_Heater_current = {
        TableName: 'GreenhouseDatabase',
        Item: {
            'reported': { S: 'Heater_current' },
            'index': { S: '0001' },
            'val': { N: Heater_current.toString() }
        }
    };
    ddb.putItem(params_Heater_current, function (err, data) {
        if (err) {
            console.log("Error Writing Heater_current", err);
        } else {
            console.log("Success Writing Heater_current");
        }
    });
}

function Send_Update_Message(IOT_data, context, Last_time, Specific_time, Heater_runningtime_total, Month) {
    var DateYMD = IOT_data[0];
    var Time = IOT_data[1];
    var Temperature_inside = IOT_data[2];
    var Temperature_outside = IOT_data[3];
    var Humidmity = IOT_data[4];
    var Light_level = IOT_data[5];
    var Soil_Moisture_1 = IOT_data[6];
    var Soil_Moisture_2 = IOT_data[7];
    var Soil_Moisture_3 = IOT_data[8];
    var message1 = 'Current Time: ' + DateYMD + '-' + Time + '\n';
    var message2 = 'The Temperature inside the greenhouse is: ' + Temperature_inside + ' degrees.\n';
    var message3 = 'The Temperature outside the greenhouse is: ' + Temperature_outside + ' degrees.\n';
    var message4 = 'The Humidity inside the greenhouse is ' + Humidmity + ' %' + '\n';
    var message5 = 'The Light level inside the greenhouse is ' + Light_level + '\n';
    var message6 = 'Soil moisture 1 is ' + Soil_Moisture_1 + '\n';
    var message7 = 'Soil moisture 2 is ' + Soil_Moisture_2 + '\n';
    var message8 = 'Soil moisture 3 is ' + Soil_Moisture_3 + '\n';
    var message9 = 'The total power consumption in ' + Month + ' is ' + Heater_runningtime_total * 120 / 3600000 + ' Kwh';
    var messageupdate = message1 + message2 + message3 + message4 + message5 + message6 + message7 + message8 + message9;




    var sns = new AWS.SNS();
    var params = {
        Message: messageupdate,
        TopicArn: "arn:aws:sns:us-east-1:812365191913:GreenhouseTemAlert"
    };
    var Specific_time_int = Specific_time * 1;
    if (Specific_time_int > Last_time && Specific_time_int < Time) {
        console.log('Sending update message');
        sns.publish(params, context.done);
    }

    //SendMessage-END
}

function Switch_On() {
    const option = 'https://maker.ifttt.com/trigger/ec464greenhouseON/with/key/cvJYmevJ910Cxw7Zr5Y6Ac';
    // request(option, function (error, response, body) {
    //     console.log('error:', error); // Print the error if one occurred
    //     console.log('statusCode:', response && response.statusCode); // Print the response status code if a response was received
    //     console.log('body:', body); // Print the HTML for the Google homepage.
    // });
}

function Switch_OFF() {
    const option = 'https://maker.ifttt.com/trigger/ec464greenhouseOFF/with/key/cvJYmevJ910Cxw7Zr5Y6Ac';
    // request(option, function (error, response, body) {
    //     console.log('error:', error); // Print the error if one occurred
    //     console.log('statusCode:', response && response.statusCode); // Print the response status code if a response was received
    //     console.log('body:', body); // Print the HTML for the Google homepage.
    // });
}

function Switching_On_OFF(IOT_data, Lowest_temperature, Highest_temperature, Heater_current) {
    var Temperature_inside = IOT_data[2];
    var Heater_next;
    if (Temperature_inside < Lowest_temperature && Heater_current == 0) {
        Switch_On();
        console.log('Switch Heater On');

        Heater_next = 1;
        Write_Heater_current(Heater_next);
        Send_Low_Temperature_Message(Temperature_inside);
    }
    else if (Temperature_inside > Lowest_temperature && Heater_current == 1) {
        Switch_OFF();
        console.log('Switch Heater Off');
        Heater_next = 0;
        Write_Heater_current(Heater_next);
    }
    else if (Temperature_inside > Highest_temperature) {
        Send_High_Temperature_Message(Temperature_inside);
    }
}


function Send_Low_Temperature_Message(Temperature_inside) {
    var messagewarning_temperature_too_low = 'Warnning: the current temperature in your greenhouse is ' + Temperature_inside;
    var sns = new AWS.SNS();
    var params = {
        Message: messagewarning_temperature_too_low,
        TopicArn: "arn:aws:sns:us-east-1:812365191913:GreenhouseTemAlert"
    };
    sns.publish(params, function (err, data) {
        if (err) console.log(err, err.stack); // an error occurred
        else console.log('Sending Low Temperature Message');           // successful response
    });
}

function Send_High_Temperature_Message(Temperature_inside) {
    var messagewarning_temperature_too_high = 'Warnning: the current temperature in your greenhouse is ' + Temperature_inside;
    var sns = new AWS.SNS();
    var params = {
        Message: messagewarning_temperature_too_high,
        TopicArn: "arn:aws:sns:us-east-1:812365191913:GreenhouseTemAlert"
    };
    sns.publish(params, function (err, data) {
        if (err) console.log(err, err.stack); // an error occurred
        else console.log('Sending High Temperature Message');           // successful response
    });
}

function Count_Heater(Month, Heater_current, Last_time, IOT_data, Heater_runningtime, callback) {
    var Time = parseInt(IOT_data[1], 10);
    var Heater_runningtime_total = Heater_runningtime;

    var Time_string = Time.toString();
    var Time_char = Time_string.split('');
    var Time_converted = (Time_char[0] + Time_char[1]) * 3600;
    Time_converted += (Time_char[2] + Time_char[3]) * 60;
    Time_converted += (Time_char[4] + Time_char[5]) * 1;

    var Last_time_string = Last_time.toString();
    var Last_time_char = Last_time_string.split('');
    var Last_time_converted = (Last_time_char[0] + Last_time_char[1]) * 3600;
    Last_time_converted += (Last_time_char[2] + Last_time_char[3]) * 60;
    Last_time_converted += (Last_time_char[4] + Last_time_char[5]) * 1;

    console.log(Time);
    console.log(Last_time);
    console.log(Time - Last_time);
    console.log(Time_converted - Last_time_converted);
    if (Heater_current == 1) {
        Heater_runningtime_total = Heater_runningtime + (Time_converted - Last_time_converted);
        Update_Heater_runningtime(Month, Heater_runningtime_total);
    }
    return callback(Heater_runningtime_total);
}

function Update_Heater_runningtime(Month, Heater_runningtime_total) {
    var params_Heater_runningtime = {
        TableName: 'GreenhouseDatabase',
        Item: {
            'reported': { S: 'Power_consumption_' + Month },
            'index': { S: '0001' },
            'val': { N: Heater_runningtime_total.toString() }
        }
    };
    ddb.putItem(params_Heater_runningtime, function (err, data) {
        if (err) {
            console.log("Error Writing Heater_runningtime", err);
        } else {
            console.log("Success Writing Heater_runningtime");
        }
    });
}

function Read_Heater_runningtime(Month, callback) {
    var params_Heater_runningtime = {
        TableName: 'GreenhouseDatabase',
        Key: {
            'reported': { S: 'Power_consumption_' + Month },
            'index': { S: '0001' },
        }
    };
    ddb.getItem(params_Heater_runningtime, function (err, data, response) {
        if (err) {
            console.log("Error Reading Heater_runningtime", err);
        }
        else {
            console.log("Success Reading Heater_runningtime");
            var Jsondata = JSON.stringify(data, null, 2);
            var Heater_runningtime = JSON.parse(Jsondata).Item.val.N;
            return callback(Heater_runningtime);
        }
    });
}

function Decide_month(IOT_data, callback) {
    var DateYMD = IOT_data[0].toString();
    var DateYMD_chars = DateYMD.split('');
    var Month_number = DateYMD_chars[4] + DateYMD_chars[5];
    var Month;
    switch (Month_number) {
        case '01':
            Month = 'Jan';
            break;
        case '02':
            Month = 'Feb';
            break;
        case '03':
            Month = 'Mar';
            break;
        case '04':
            Month = 'Apr';
            break;
        case '05':
            Month = 'May';
            break;
        case '06':
            Month = 'Jun';
            break;
        case '07':
            Month = 'Jul';
            break;
        case '08':
            Month = 'Aug';
            break;
        case '09':
            Month = 'Sep';
            break;
        case '10':
            Month = 'Oct';
            break;
        case '11':
            Month = 'Nov';
            break;
        case '12':
            Month = 'Dec';
            break;
        default:
            console.log('Wrong Month Number');
    }
    return callback(Month);

}


function Read_Interval(callback) {
    var params_Last_time = {
        TableName: 'GreenhouseDatabase',
        Key: {
            'reported': { S: 'Interval' },
            'index': { S: '0001' },
        }
    };
    ddb.getItem(params_Last_time, function (err, data, response) {
        if (err) {
            console.log("Error Reading Interval", err);
        }
        else {
            console.log("Success Reading Interval");
            var Jsondata = JSON.stringify(data, null, 2);
            var Interval = JSON.parse(Jsondata).Item.val.N;
            return callback(Interval);
        }
    });
}

function Write_Interval(Interval) {
    var params_Interval = {
        TableName: 'GreenhouseDatabase',
        Item: {
            'reported': { S: 'Interval' },
            'index': { S: '0001' },
            'val': { N: Interval.toString() }
        }
    };
    ddb.putItem(params_Interval, function (err, data) {
        if (err) {
            console.log("Error Writing Interval", err);
        } else {
            console.log("Success Writing Interval");
        }
    });
}