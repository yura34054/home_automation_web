import json

from django.http import HttpResponse
from django.shortcuts import render
from django.views.decorators.csrf import csrf_exempt

from app.internal.models import SensorReading
from app.internal.services import sensor_service


def index(request):
    context = {"sensor_reading": SensorReading.objects.first()}
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
