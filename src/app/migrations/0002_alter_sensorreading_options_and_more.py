# Generated by Django 4.2.9 on 2024-01-05 12:00

from django.db import migrations, models


class Migration(migrations.Migration):
    dependencies = [
        ("app", "0001_initial"),
    ]

    operations = [
        migrations.AlterModelOptions(
            name="sensorreading",
            options={"ordering": ("created_on",)},
        ),
        migrations.RenameField(
            model_name="sensor",
            old_name="controller_name",
            new_name="name",
        ),
        migrations.AlterField(
            model_name="sensorreading",
            name="nox_index",
            field=models.IntegerField(),
        ),
        migrations.AlterField(
            model_name="sensorreading",
            name="voc_index",
            field=models.IntegerField(),
        ),
    ]
