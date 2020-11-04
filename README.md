# sensorsDataTransfer




< insert the diagram here >







## Arduino :
  ## Les fichiers
  - La carte arduino a une carte SD. Cette carte arduino aura 4 fichiers. 
    - data : contient les données des 7 derniers jours
    - Log : contient la dernière date qui a été ajouter dans la base de données
    - Alarme : contient les alarme qui peut etre declanché. 
    - config : contient les configurations pour le fonctionnement du dispositif
      - le nombre de jour à sauvegarder
      - l'interval de temps entre chaque mesure. 
  
  ## Les execptions
  Suite aux mesures, des exceptions peuvent être lancé: 
  - Si les valeurs du capteurs existent mais que aucune valeurs pour le GPS il met 0,00000; 0,00000 pour les coordonnées
  - Si les valeurs du capteurs n'existent pas mais que les valeurs du GPS exitsent, au bout de 10 secondes, une alarme et lancé 
  - Si ni les valeurs du capteur, ni les valeurs du GPS n'existent une alarme doit être lancé. 
    
   Les alarmes sont ecritent dans le fichier `alarme`
   
   ## Suppression des données en dehors de la plage de la semaine
   Un processus va permettre de supprimer les données du fichier `data` si TS(data) - TS(log) > 7. 
   Pour la suppression on pourrai utilser un fichier temporaire
   - Recopier toutes les données comprises dans cette plage nommée `temp`
   - Supprimer le fichier `data`
   - Renommer le fichier `temp` en `data`
   
   ## Pour la V2
   Un capteur de CO2 va etre placer sur la carte, on va devoir trouvé une facon de derterminer si le capteur de CO2 est integrer sur la carte.

## Application android : 

Insertion dans la base de données :
Le fichier log contient la date de la dernière  donnée du capteur
- Si TS(actuelle) - TS (log) > 7 :
Aucune donnée sur la carte SD, il faut aller la date de la première mesure enregistrer sur la carte SD. 
- Si TS(actuelle) - TS(log) <= 7 :
Chercher les valeurs qui sont entre TS(log) et TS(actuelle)


