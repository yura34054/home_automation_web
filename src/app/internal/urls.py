from django.urls import path

from app.internal.transport import chart_views, views

urlpatterns = [
    path("", views.index, name="index"),
    path("co2_chart", chart_views.co2_chart, name="co2_chart"),
    path("api/create_sensor_reading", views.create_sensor_reading),
]
