from django.utils import timezone
from sensirion_gas_index_algorithm.nox_algorithm import NoxAlgorithm
from sensirion_gas_index_algorithm.voc_algorithm import VocAlgorithm

from app.internal.models import Sensor, SensorReading


class SensorReadingGenerator:
    def __init__(self):
        self.last_update = timezone.now()
        self.voc_algorithm = VocAlgorithm()
        self.nox_algorithm = NoxAlgorithm()

    def update(
        self,
        controller_name: str,
        api_key: str,
        temperature,
        humidity,
        carbon_dioxide,
        raw_voc_data: [int],
        raw_nox_data: [int],
    ):
        sensor = Sensor.objects.get(controller_name=controller_name)
        if sensor.api_key != api_key:
            return False

        # reset algorithms if more than 10 minutes passed
        if timezone.now() - self.last_update >= timezone.timedelta(minutes=10):
            self.voc_algorithm = VocAlgorithm()
            self.nox_algorithm = NoxAlgorithm()

        # add voc and nox data to algorithms
        for s_voc_raw, s_nox_raw in zip(raw_voc_data, raw_nox_data):
            voc_index = self.voc_algorithm.process(s_voc_raw)
            nox_index = self.nox_algorithm.process(s_nox_raw)

        # this is a horrible bodge to write to db only once a minute, it brakes sooo many rules
        # but IDK how to do this better (
        if timezone.now() - self.last_update < timezone.timedelta(minutes=1):
            return True

        SensorReading.objects.create(
            sensor=sensor,
            temperature=round(temperature, 2),
            humidity=round(humidity, 2),
            carbon_dioxide=carbon_dioxide,
            voc_raw=round(sum(raw_voc_data) / len(raw_voc_data), 2),
            nox_raw=round(sum(raw_nox_data) / len(raw_nox_data), 2),
            voc_index=voc_index,
            nox_index=nox_index,
        )
        self.last_update = timezone.now()

        return True
