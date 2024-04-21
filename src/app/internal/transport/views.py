import json

import plotly.express as px
from django.http import HttpResponse
from django.shortcuts import render
from django.utils import timezone
from django.views.decorators.csrf import csrf_exempt

from app.internal.models import Sensor, SensorReading
from app.internal.services import sensor_service

readings_dict = {
    "carbon_dioxide_chart": ("carbon_dioxide", "CO2 PPM"),
    "humidity_chart": ("humidity", "Relative humidity %"),
    "temperature_chart": ("temperature", "Temperature C"),
    "voc_index_chart": ("voc_index", "VOC index"),
    "nox_index_chart": ("nox_index", "NOx index"),
    "pm1_0_chart": ("pm1_0", "PM1.0"),
    "pm2_5_chart": ("pm2_5", "PM2.5"),
    "pm10_chart": ("pm10", "PM10"),
}


def index(request):
    context = {"sensor_list": Sensor.objects.all()}
    return render(request, "index.html", context)


@csrf_exempt
def create_sensor_reading(request):
    if request.method != "POST":
        return HttpResponse(status=405)

    data = json.loads(request.body)

    status = sensor_service.create_sensor_reading(
        controller_name=data["controller_name"],
        api_key=data["api_key"],
        temperature=data["temperature"],
        humidity=data["humidity"],
        carbon_dioxide=data["carbon_dioxide"],
        voc_index=data["voc_index"],
        nox_index=data["nox_index"],
        pm1_0=data["pm1.0"],
        pm2_5=data["pm2.5"],
        pm10=data["pm10"],
    )

    if status:
        return HttpResponse(status=201)

    return HttpResponse(status=401)


def render_chart(request, sensor_name, chart_name):
    if not Sensor.objects.filter(name=sensor_name).exists():
        return HttpResponse(f"no sensor {sensor_name} found", status=404)

    if chart_name not in readings_dict:
        return HttpResponse(f"no chart {chart_name} found", status=404)

    field_name, title = readings_dict[chart_name]

    readings = SensorReading.objects.filter(
        created_on__gte=timezone.now() - timezone.timedelta(hours=2), sensor__name=sensor_name
    ).values_list("created_on", field_name)

    if len(readings) < 2:
        return HttpResponse("not enough readings", status=404)

    fig = px.line(
        x=[c[0] for c in readings],
        y=[c[1] for c in readings],
        title=title,
        labels={"x": "Time", "y": title},
    )

    fig.update_layout(title={"font_size": 24, "xanchor": "center", "x": 0.5})
    html_chart = fig.to_html()
    context = {"chart": html_chart}
    return render(request, "chart.html", context)
