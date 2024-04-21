from django.urls import path

from app.internal.transport import views

urlpatterns = [
    path("", views.index, name="index"),
    path("api/create_sensor_reading", views.create_sensor_reading),
    path("<str:sensor_name>/<str:chart_name>", views.render_chart, name="render_chart"),
]
