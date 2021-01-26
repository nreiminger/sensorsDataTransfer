# sensorsDataTransfer




![diagramme](https://github.com/nreiminger/sensorsDataTransfer/blob/main/Arduino_Android_diagramm.png)



## Arduino :
  ## Les fichiers
  - La carte arduino a une carte SD. Cette carte arduino aura 4 fichiers. 
    - data : contient les données des 7 derniers jours
    - config : contient les configurations pour le fonctionnement du dispositif (lecture du fichier )
      - l'id du capteur, pour les tests c'est le dpolhin 7 qui a été tester
      - l'interval de temps entre chaque mesure. 
  
  ## Les execptions
  Suite aux mesures, des exceptions peuvent être lancé: 
  - Si les valeurs du capteurs existent mais que aucune valeurs pour le GPS il met 0,00000; 0,00000 pour les coordonnées
  - Si les valeurs du capeteurs existent que la date/heure renseigné par le GPS sont invalide ou egale à `0/0/2000`/`0:0:0` on prendra comme date la valeur de millis fourni par Arduino.
  - Si les données conecerant la polution pour les pm1, pm2_5 et pm10 sont toute les 3 égale a 0 nous conciderons que cette valeur n'est pas correct donc elle n'est même pas enregistrer sur la carte SD.
  
 ## Suppression des données en dehors de la plage de la semaine ( je ne sais pas encore comment proceder puisque à l'intérieur les valeurs du gps concernant la date/heure sont invalide) 
   Un processus va permettre de supprimer les données du fichier `data` si TS(data) - TS(log) > 7. 
   Pour la suppression on pourrai utilser un fichier temporaire
   - Recopier toutes les données comprises dans cette plage nommée `temp`
   - Supprimer le fichier `data`
   - Renommer le fichier `temp` en `data`
   
   ## Pour la V2
   Un capteur de CO2 va etre placer sur la carte, on va devoir trouvé une facon de derterminer si le capteur de CO2 est integrer sur la carte.

## Application android : 

Insertion dans la base de données :
  - A l'allumage de l'application, lorsque le téléphone se connecte à la carte Arduino la valeur de epoch() (date courante) est notée dans les sharePréfences, cette donnée va permettre de recalculer la date d'une mesure si la date fourni par le GPS est invalide. 
  - Chaque données, sont ensuite lu de la plus recente à la plus an ancienne. Chaque ligne de la carte SD, représante chaqu'une mesure mais sont rengé de la plus recente ancienne a la plus ancienne, donc il faut renverser le fichier. En placant que ligne dans une List, la date la plus recente ce trouve ensuite au debut sommet de la List. 
  - Chaque composante de chaque ligne sont ensuiet envoyé a l'application mobile, précédée par un préfixe permettant de les reperée ("d=","h=","pm=","lt=","lg=","m="). A la reception de chaque composante, il sont placé dans un objet permettant de les saugarder jusqu'a l'envoye de la requete. La seul variable non envoyé, la valeur du millis, préfixé par "m=" sera utilsé pour recalculé, a l'aide de la date courante enregistrer dans le localStorage, la date si celle ci n'est pas correct, ou quel n'est pas vide. Si cette valeur est null cela signifie que nous somme directement connecté avec le téléphone donc nous utiliserons la date courante du téléphone.   

