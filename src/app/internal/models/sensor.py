from django.db import models


class Sensor(models.Model):
    name = models.fields.CharField(max_length=64, primary_key=True)
    api_key = models.CharField(max_length=255, blank=True)

    def __str__(self):
        return self.name
