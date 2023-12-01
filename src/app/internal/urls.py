from django.urls import path

from app.internal.transport import views

urlpatterns = [path("", views.index, name="index"), path("api/crate_sensor_reading", views.crate_sensor_reading)]
