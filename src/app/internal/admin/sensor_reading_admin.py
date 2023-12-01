from django.contrib import admin

from app.internal.models.sensor_reading import SensorReading


@admin.register(SensorReading)
class SensorReadingAdmin(admin.ModelAdmin):
    pass
