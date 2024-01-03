from django.db import models


class SensorReading(models.Model):
    sensor = models.ForeignKey(
        "Sensor",
        on_delete=models.CASCADE,
    )
    created_on = models.DateTimeField(auto_now_add=True)

    temperature = models.FloatField()
    humidity = models.FloatField()
    carbon_dioxide = models.IntegerField()

    voc_raw = models.FloatField()
    nox_raw = models.FloatField()
    voc_index = models.FloatField()
    nox_index = models.FloatField()

    def __str__(self):
        return f"{self.sensor}: {self.created_on}"
