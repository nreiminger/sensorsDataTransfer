# sensorsDataTransfer




< insert the diagram here >







## Arduino :
  ## Les fichiers
  - La carte arduino a une carte SD. Cette carte arduino aura 4 fichiers. 
    - data : contient les données des 7 derniers jours
    - Log : contient la dernière date qui a été ajouter dans la base de données
    - Alarme : contient les alarme qui peut etre declanché. 
    - config : contient les configurations pour le fonctionnement du dispositif, le nombre de jour à sauvegarder, l'interval de temps entre chaque mesure. 
  
  ## Les execptions



## Application android : 



Insertion dans la base de données :
Le fichier log contient la date de la dernière  donnée du capteur
Si TS(actuelle) - TS (log) > 7 :
Aucune donnée sur la carte SD, il faut aller la date de la première mesure enregistrer sur la carte SD. 
Si TS(actuelle) - TS(log) <= 7 :
Chercher les valeurs qui sont entre TS(log) et TS(actuelle)


