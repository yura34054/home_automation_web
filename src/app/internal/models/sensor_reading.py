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
    carbon_dioxide = models.IntegerField()

    voc_raw = models.FloatField()
    nox_raw = models.FloatField()
    voc_index = models.IntegerField()
    nox_index = models.IntegerField()

    def __str__(self):
        return f"{self.sensor}: {self.created_on}"

    class Meta:
        ordering = ('created_on',)
