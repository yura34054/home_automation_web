from django.contrib import admin

from app.internal.models.sensor import Sensor


@admin.register(Sensor)
class SensorAdmin(admin.ModelAdmin):
    pass
