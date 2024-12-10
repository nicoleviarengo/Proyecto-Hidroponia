from django.contrib import admin
from django.urls import path
from . import views

urlpatterns = [
    path('', views.web, name='web'),
    path('miscultivos/', views.mis_cultivos, name='mis_cultivos'),
    path('guardar_cultivo/', views.guardar_cultivo, name='guardar_cultivo'),
    path('actualizar_cultivo/<int:id>/', views.actualizar_cultivo, name='actualizar_cultivo'),  #estoy cambiando actualizar/<int:id>/' 
    path('eliminar_cultivo/<int:id>/', views.eliminar_cultivo, name='eliminar_cultivo'), #aca hice lo mismo
]