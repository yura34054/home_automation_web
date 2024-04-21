from django.db import models
from django.utils import timezone


class SensorReading(models.Model):
    sensor = models.ForeignKey(
        "Sensor",
        on_delete=models.CASCADE,
    )
    created_on = models.DateTimeField(default=timezone.now)

    temperature = models.FloatField()
    humidity = models.FloatField()
    carbon_dioxide = models.FloatField()

    voc_index = models.IntegerField()
    nox_index = models.IntegerField()

    pm1_0 = models.FloatField(verbose_name="PM1.0")
    pm2_5 = models.FloatField(verbose_name="PM2.5")
    pm10 = models.FloatField(verbose_name="PM10")

    def __str__(self):
        return f"{self.sensor}: {self.created_on}"

    class Meta:
        ordering = ("-created_on",)
