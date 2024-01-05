from django.urls import path

from app.internal.transport import chart_views, views

urlpatterns = [
    path("", views.index, name="index"),
    path("api/create_sensor_reading", views.create_sensor_reading),
    path("<str:sensor_name>/carbon_dioxide_chart", chart_views.carbon_dioxide_chart, name="carbon_dioxide_chart"),
    path("<str:sensor_name>/humidity_chart", chart_views.humidity_chart, name="humidity_chart"),
    path("<str:sensor_name>/temperature_chart", chart_views.temperature_chart, name="temperature_chart"),
    path("<str:sensor_name>/voc_index_chart", chart_views.voc_index_chart, name="voc_index_chart"),
    path("<str:sensor_name>/nox_index_chart", chart_views.nox_index_chart, name="nox_index_chart"),
]
