import json

from django.http import HttpResponse
from django.shortcuts import render
from django.views.decorators.csrf import csrf_exempt

from app.internal.services import sensor_service


def index(request):
    return HttpResponse(render(request, "index.html"))


@csrf_exempt
def crate_sensor_reading(request):
    if request.method != "POST":
        return HttpResponse(status=405)

    data = json.loads(request.body)

    status = sensor_service.crate_sensor_reading(
        name=data["name"],
        api_key=data["api_key"],
        temperature=data["temperature"],
        humidity=data["humidity"],
        carbon_dioxide=data["carbon_dioxide"],
    )

    if status:
        return HttpResponse(status=201)

    return HttpResponse(status=401)
