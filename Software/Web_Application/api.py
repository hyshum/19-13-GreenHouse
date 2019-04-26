from __future__ import print_function # Python 2/3 compatibility
import urllib.request
import os
import ssl
import json
import boto3
from boto3.dynamodb.conditions import Key, Attr
import decimal
import datetime
from datetime import timedelta

# Helper class to convert a DynamoDB item to JSON.
class DecimalEncoder(json.JSONEncoder):
    def default(self, o):
        if isinstance(o, decimal.Decimal):
            if o % 1 > 0:
                return float(o)
            else:
                return int(o)
        return super(DecimalEncoder, self).default(o)

gcloud_api_key=os.environ["GCLOUD_KEY"]
context = ssl._create_unverified_context()
google_api_root="https://maps.googleapis.com/maps/api/"

def get_distance_matrix(origin, destination, units="metric", languages="en"):
    params = [
        ("units",units),
        ("origins", origin),
        ("destinations", destination),
        ("key", gcloud_api_key),
        ("language", languages)
    ]
    url = google_api_root + "distancematrix/json?" + urllib.parse.urlencode(params)
    contents = urllib.request.urlopen(url, context=context).read()
    data = json.loads(contents.decode('utf-8'))
    return data

def get_item():
    # Get the service resource.
    dynamodb = boto3.resource('dynamodb')

    # Instantiate a table resource object without actually
    # creating a DynamoDB table. Note that the attributes of this table
    # are lazy-loaded: a request is not made nor are the attribute
    # values populated until the attributes
    # on the table resource are accessed or its load() method is called.
    table = dynamodb.Table('GreenhouseDatabase')

    # Print out some data about the table.
    # This will cause a request to be made to DynamoDB and its attribute
    # values will be set based on the response.
    print(table.creation_date_time)

    response = table.get_item(
    Key={
        "index": "1551215105717",
        "reported": "$aws/things/Greenhouse_project/shadow/update"
    }
    )
    item = response['Item']
    print(item)
    return item

def scan():
    #https://docs.aws.amazon.com/amazondynamodb/latest/developerguide/GettingStarted.Python.04.html#GettingStarted.Python.04.Query.01
    dynamodb = boto3.resource('dynamodb')

    table = dynamodb.Table('GreenhouseDatabase')

    #print(table.creation_date_time)
    now = datetime.datetime.now()
    today = now.year * 10000 + now.month * 100 + now.day
    #past = datetime.datetime.now() - timedelta(hours = 6)
    #past_bound = str(past.hour) + str(past.minute) + str(past.second)

    #print(past)
    #print(past_bound)
    response = table.scan(
        FilterExpression=Attr('Date').gte(today) # & Attr('Time').gte(past_bound)
    )
    #all items from the last past 6 hours.
    items = response['Items']
    print(items)
    return items

def update(desired_low = 55, desired_high = 75, desired_notification_time = '1623'):

    dynamodb = boto3.resource('dynamodb')
    table = dynamodb.Table('GreenhouseDatabase')

    table.update_item(
        Key={
            'reported': 'Lowest_temperature',
            'index': '0002'
        },
        UpdateExpression='SET val = :val1',
        ExpressionAttributeValues={
        ':val1': desired_low
        }
    )

    table.update_item(
        Key={
            'reported': 'Highest_temperature',
            'index': '0002'
        },
        UpdateExpression='SET val = :val1',
        ExpressionAttributeValues={
        ':val1': desired_high
        }
    )

    table.update_item(
        Key={
            'reported': 'Specific_time',
            'index': '0002'
        },
        UpdateExpression='SET val = :val1',
        ExpressionAttributeValues={
        ':val1': desired_notification_time
        }
    )

    return

def get_state_on():
    dynamodb = boto3.resource('dynamodb')
    table = dynamodb.Table('GreenhouseDatabase')

    response = table.get_item(
        Key={
            "index": "0001",
            "reported": "Heater_current"
    }
    )
    state_string = "Off"
    current_state = int(response['Item']['val'])
    if current_state == 1:
        state_string = "On"
    #print('what is current state')
    #print(type(current_state))
    #print(current_state)

    return state_string

def get_state_current_low():
    dynamodb = boto3.resource('dynamodb')
    table = dynamodb.Table('GreenhouseDatabase')

    response = table.get_item(
        Key={
            "index": "0002",
            "reported": "Lowest_temperature"
    }
    )
    current_low = 20
    current_low = int(response['Item']['val'])
    #print('what is current state')
    #print(type(current_state))
    #print(current_state)

    return current_low

def get_state_current_high():
    dynamodb = boto3.resource('dynamodb')
    table = dynamodb.Table('GreenhouseDatabase')

    response = table.get_item(
        Key={
            "index": "0002",
            "reported": "Highest_temperature"
    }
    )
    current_high = 20
    current_high = int(response['Item']['val'])
    #print('what is current state')
    #print(type(current_state))
    #print(current_state)

    return current_high

def get_state_power_consumption():
    dynamodb = boto3.resource('dynamodb')
    table = dynamodb.Table('GreenhouseDatabase')

    response = table.get_item(
        Key={
            "index": "0001",
            "reported": "Power_consumption_Apr"
    }
    )
    kwh = 0
    kwh = response['Item']['val']
    #print('what is current state')
    #print(type(current_state))
    #print(current_state)

    return kwh

def get_energy_used():
    dynamodb = boto3.resource('dynamodb')
    table = dynamodb.Table('GreenhouseDatabase')
    monthly_energy = []

    response1 = table.get_item(
        Key={
            "index": "0001",
            "reported": "Power_consumption_Jan"
    }
    )
    monthly_energy.append(int(response1['Item']['val']))

    response2 = table.get_item(
        Key={
            "index": "0001",
            "reported": "Power_consumption_Feb"
    }
    )
    monthly_energy.append(int(response2['Item']['val']))

    response3 = table.get_item(
        Key={
            "index": "0001",
            "reported": "Power_consumption_Mar"
    }
    )
    monthly_energy.append(int(response3['Item']['val']))

    response4 = table.get_item(
        Key={
            "index": "0001",
            "reported": "Power_consumption_Apr"
    }
    )
    monthly_energy.append(int(response4['Item']['val']))

    response5 = table.get_item(
        Key={
            "index": "0001",
            "reported": "Power_consumption_May"
    }
    )
    monthly_energy.append(int(response5['Item']['val']))

    response6 = table.get_item(
        Key={
            "index": "0001",
            "reported": "Power_consumption_Jun"
    }
    )
    monthly_energy.append(int(response6['Item']['val']))

    response7 = table.get_item(
        Key={
            "index": "0001",
            "reported": "Power_consumption_Jul"
    }
    )
    monthly_energy.append(int(response7['Item']['val']))


    response8 = table.get_item(
        Key={
            "index": "0001",
            "reported": "Power_consumption_Aug"
    }
    )
    monthly_energy.append(int(response8['Item']['val']))

    response9 = table.get_item(
        Key={
            "index": "0001",
            "reported": "Power_consumption_Sep"
    }
    )
    monthly_energy.append(int(response9['Item']['val']))

    response10 = table.get_item(
        Key={
            "index": "0001",
            "reported": "Power_consumption_Oct"
    }
    )
    monthly_energy.append(int(response10['Item']['val']))

    response11 = table.get_item(
        Key={
            "index": "0001",
            "reported": "Power_consumption_Nov"
    }
    )
    monthly_energy.append(int(response11['Item']['val']))

    response12 = table.get_item(
        Key={
            "index": "0001",
            "reported": "Power_consumption_Dec"
    }
    )
    monthly_energy.append(int(response12['Item']['val']))

    return monthly_energy

def get_notification_time():
    dynamodb = boto3.resource('dynamodb')
    table = dynamodb.Table('GreenhouseDatabase')
    minute_string = '12'
    hour_string = '00'
    response = table.get_item(
        Key={
            "index": "0002",
            "reported": "Specific_time"
    }
    )

    hour_string = str(response['Item']['val'])[0:2]
    minute_string = str(response['Item']['val'])[2:4]
    time_string = str(response['Item']['val'])

    time_list = [hour_string, minute_string, time_string]
    return time_list
