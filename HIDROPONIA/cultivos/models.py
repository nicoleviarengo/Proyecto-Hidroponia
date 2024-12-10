from django.db import models
from django.utils import timezone
import datetime

class Cultivo(models.Model):
    id = models.BigAutoField(primary_key=True)
    nombre = models.CharField(max_length=45)
    tipo = models.CharField(max_length=45, default='General')
    cuandoSeCultiva = models.CharField(max_length=45)
    cuidados = models.CharField(max_length=100)
    comentarios = models.CharField(max_length=100)

    def __str__(self):
        return self.nombre

class Parametros(models.Model):
    id = models.AutoField(primary_key=True)
    humedadSuelo_min = models.FloatField(default=0.0)
    humedadSuelo_max = models.FloatField(default=0.0)
    humedadAmbiente_min = models.FloatField(default=0.0)
    humedadAmbiente_max = models.FloatField(default=0.0)
    temperaturaAmbiente_min = models.FloatField(default=0.0)
    temperaturaAmbiente_max = models.FloatField(default=40.0)
    luminiscencia_min = models.FloatField(default=0.0)
    luminiscencia_max = models.FloatField(default=100.0)
    cultivo = models.ForeignKey(Cultivo, on_delete=models.CASCADE, related_name='params', null=True, blank=True)

class Resumen(models.Model):
    id = models.AutoField(primary_key=True)
    fecha = models.DateTimeField(default=timezone.now)
    dispositivo = models.CharField(max_length=45, default='pordefecto')
    luz = models.FloatField(default=0.0)
    humedad = models.FloatField(default=0.0)
    temperatura = models.FloatField(default=25.0)
    humedadSuelo = models.FloatField(default=0.0)
    rendimiento = models.FloatField(default=0.0)
    #cultivo = models.ForeignKey(Cultivo, on_delete=models.CASCADE, related_name='resumenes', null=True, blank=True)

    # def __str__(self):
    #     if self.cultivo:
    #         return f"Resumen del {self.fecha} para {self.cultivo.nombre}"
    #     return f"Resumen del {self.fecha} (sin cultivo asociado)"



