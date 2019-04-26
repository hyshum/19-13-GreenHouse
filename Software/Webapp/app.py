from flask import Flask, request, Response, render_template
import os.path
import api
import json
import decimal

def root_dir():  # pragma: no cover
    return os.path.abspath(os.path.dirname(__file__))

def get_file(filename):  # pragma: no cover
    try:
        src = os.path.join(root_dir(), filename)
        # Figure out how flask returns static files
        # Tried:
        # - render_template
        # - send_file
        # This should not be so non-obvious
        return open(src).read()
    except IOError as exc:
        return str(exc)


app = Flask(__name__)

@app.route('/')
def index():
    content = get_file('static/index.html')
    return Response(content, mimetype="text/html")

@app.route('/api_demo')
def api_demo():
    content = get_file('static/api_demo.html')
    return Response(content, mimetype="text/html")

@app.route('/api_demo/result', methods=['GET'])
def api_demo_result():
    # print(request.args)
    origin = request.args.get('origin')
    destination = request.args.get('destination')
    response = api.get_distance_matrix(origin, destination)

    print(response)

    status = response['rows'][0]['elements'][0]['status']

    if status == 'ZERO_RESULTS' or status == 'NOT_FOUND':
        return 'Error finding route'

    origin_formatted = response['origin_addresses'][0]
    destination_formatted = response['destination_addresses'][0]
    distance = response['rows'][0]['elements'][0]['distance']['text']
    duration = response['rows'][0]['elements'][0]['duration']['text']

    return render_template('result.html',origin=origin_formatted, destination=destination_formatted, distance=distance, duration=duration)

@app.route('/login')
def login():
    content = get_file('static/login.html')
    return Response(content, mimetype="text/html")

@app.route('/login/dashboard', methods=['GET'])
def login_dashboard():
    on = api.get_state_on()
    current_low = api.get_state_current_low()
    current_high = api.get_state_current_high()
    #currently manually set to collect the power consumption of April (CHANGE REQUIRED FOR EACH MONTH)
    kwh = api.get_state_power_consumption()
    #cost estimiated based on 2019 Summary of Eastern Massachusetts Electric Rates: less than 10 cents distribution = $0.06396, ,,transmition = 0.02  for each kwh
    # * decimal.Decimal(0.10) * decimal.Decimal(120) * decimal.Decimal(1/3600000)
    #this kwh is actually seconds.
    estimated_cost = round(decimal.Decimal(kwh) * decimal.Decimal(0.0000033333) ,5)
    items = api.scan()
    time = []
    outside_temp = []
    inside_temp = []
    humidity = []
    light_level = []
    #soil moisture
    s1 = []
    s2 = []
    s3 = []

    #print(type(items))
    #print(type(items[0]['Humidmity']))

    for i in range(len(items)):
        time.append(int(items[i]['Time']))
        outside_temp.append(int(items[i]['Temperature_outside']))
        inside_temp.append(int(items[i]['Temperature_inside']))
        humidity.append(int(items[i]['Humidmity']))
        s1.append(int(items[i]['Soil_Moisture_1']))
        s2.append(int(items[i]['Soil_Moisture_2']))
        s3.append(int(items[i]['Soil_Moisture_3']))
        light_level.append(int(items[i]['Light_level']))

    #Get kwh for each monthly_energy month for 12 months: [100,221,22311,...]
    monthly_energy = api.get_energy_used()
    print("Here is the monthly energy type")
    print(type(monthly_energy[0]))

    monthly_cost = []
    for i in range(len(monthly_energy)):
        #monthly_cost.append(int(round(decimal.Decimal(monthly_energy[i]) * decimal.Decimal(0.10),2)))
        monthly_cost.append(int(round(decimal.Decimal(monthly_energy[i]) * decimal.Decimal(0.33333),5)))

    #get current notification time
    current_notification_time_string_list = api.get_notification_time()
    current_notification_hour = current_notification_time_string_list[0]
    current_notification_minute = current_notification_time_string_list[1]
    current_notification_time = current_notification_time_string_list[2]







    ron_label = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,2400000000000000000000000]
    print(type(ron_label[0]))
    print(outside_temp)
    return render_template('dashboard_result.html',temperature_threshold = 60, heater_state = 'On',
     ron_label= ron_label,
     time = time, outside_temp = outside_temp, inside_temp = inside_temp, humidity = humidity , s1 = s1, s2 = s2, s3 = s3, light_level = light_level
     , on = on , current_low = current_low, current_high = current_high, estimated_cost = estimated_cost
     ,monthly_energy = monthly_energy, monthly_cost = monthly_cost
     ,current_notification_hour = current_notification_hour, current_notification_minute = current_notification_minute, current_notification_time = current_notification_time)

@app.route('/db_demo')
def db_demo():
    item = api.scan()
    content = get_file('static/login.html')
    return Response(content, mimetype="text/html")

@app.route('/login/dashboard/submit', methods=['GET'])
def sumbit_form():
    #print(request.args)


    desired_low = request.args.get('desired_low')
    desired_high = request.args.get('desired_high')
    desired_notification_time = request.args.get('desired_notification_time')

    print("This is the type of desired_low")
    print(type(desired_low))

    print('THIS IS THE TYPE OF DESIRED NOTIFICATION TIME')
    print(type(desired_notification_time))
    api.update(desired_low, desired_high, desired_notification_time)

    #return render_template('update.html',set1='Updated Low', set2='Updated High', desired_low = desired_low, desired_high = desired_high)
    return login_dashboard()
