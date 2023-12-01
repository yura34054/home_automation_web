from django.db import models


class SensorReading(models.Model):
    sensor = models.ForeignKey(
        "Sensor",
        on_delete=models.CASCADE,
    )
    created_on = models.DateTimeField(auto_now_add=True)

    temperature = models.IntegerField()
    humidity = models.IntegerField()
    carbon_dioxide = models.IntegerField()

    def __str__(self):
        return f"{self.sensor}: {self.created_on}"
