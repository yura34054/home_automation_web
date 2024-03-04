import plotly.express as px
from django.shortcuts import render
from django.utils import timezone

from app.internal.models import SensorReading


def __render_chart(request, fig):
    fig.update_layout(title={"font_size": 24, "xanchor": "center", "x": 0.5})
    html_chart = fig.to_html()
    context = {"chart": html_chart}
    return render(request, "chart.html", context)


def carbon_dioxide_chart(request, sensor_name):
    readings = SensorReading.objects.filter(
        created_on__gte=timezone.now() - timezone.timedelta(hours=2), sensor__name=sensor_name
    ).only("created_on", "carbon_dioxide")

    fig = px.line(
        x=[c.created_on for c in readings],
        y=[c.carbon_dioxide for c in readings],
        title="CO2 PPM",
        labels={"x": "Time", "y": "CO2 PPM"},
    )

    return __render_chart(request, fig)


def humidity_chart(request, sensor_name):
    readings = SensorReading.objects.filter(
        created_on__gte=timezone.now() - timezone.timedelta(hours=2), sensor__name=sensor_name
    ).only("created_on", "humidity")

    fig = px.line(
        x=[c.created_on for c in readings],
        y=[c.humidity for c in readings],
        title="humidity",
        labels={"x": "Time", "y": "Relative humidity %"},
    )

    return __render_chart(request, fig)


def temperature_chart(request, sensor_name):
    readings = SensorReading.objects.filter(
        created_on__gte=timezone.now() - timezone.timedelta(hours=2), sensor__name=sensor_name
    ).only("created_on", "temperature")

    fig = px.line(
        x=[c.created_on for c in readings],
        y=[c.temperature for c in readings],
        title="temperature",
        labels={"x": "Time", "y": "Temperature C"},
    )

    return __render_chart(request, fig)


def voc_index_chart(request, sensor_name):
    readings = SensorReading.objects.filter(
        created_on__gte=timezone.now() - timezone.timedelta(hours=2), sensor__name=sensor_name
    ).only("created_on", "voc_index")

    fig = px.line(
        x=[c.created_on for c in readings],
        y=[c.voc_index for c in readings],
        title="voc_index",
        labels={"x": "Time", "y": "VOC index"},
    )

    return __render_chart(request, fig)


def nox_index_chart(request, sensor_name):
    readings = SensorReading.objects.filter(
        created_on__gte=timezone.now() - timezone.timedelta(hours=2), sensor__name=sensor_name
    ).only("created_on", "nox_index")

    fig = px.line(
        x=[c.created_on for c in readings],
        y=[c.nox_index for c in readings],
        title="nox_index",
        labels={"x": "Time", "y": "NOx index"},
    )

    return __render_chart(request, fig)
