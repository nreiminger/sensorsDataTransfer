# sensorsDataTransfer
  IL FAUT LANCER ARDUINO EN MODE SUDO SI ON EST SUR LINUX



![diagramme](https://github.com/nreiminger/sensorsDataTransfer/blob/main/images/diagram.png)



## Arduino :
  ## Les fichiers
  - La carte arduino a une carte SD. Cette carte arduino aura 2 fichiers. 
    - data : contient les données des 7 derniers jours
    - config : contient les configurations pour le fonctionnement du dispositif (lecture du fichier )
      - l'id du capteur, pour les tests c'est le dpolhin 7 qui a été testé
      - l'interval de temps entre chaque mesure, en milliseconde, cette valeur indique le temps d'attente entre chaque mesure. 
      - la version du capteur
  
  ## Les execptions
  Suite aux mesures, des exceptions peuvent être lancé: 
  - Si les valeurs du capteurs existent mais que aucune valeurs pour le GPS il met 0,00000; 0,00000 pour les coordonnées
  - Si les valeurs du capeteurs existent que la date/heure renseigné par le GPS sont invalide ou egale à `0/0/2000`/`0:0:0` on utilisera la valeur renvoyée par la fonction millis() fourni par Arduino pour recalculer la date.
  - Si les données concernant la polution pour les pm1, pm2_5 et pm10 sont toute les 3 égale a 0 nous conciderons que cette valeur n'est pas correct donc elle n'est pas enregistrer sur la carte SD, cette valeur arrive principalement lors de l'initialisation du capteur.

## Application arduino : 
  - Lors de l'allumage de la carte Arduino, nous allons lire le fichier `config` qui contient toutes les données necessaire pour l'initialisation de BLE.
  - Par defaut, le capteur de pollution va lire des données toutes les 5 secondes, cette valeur peux varié a l'initalisation de la carte (cf fichier `config`). 
  Le capteur peux être utlisé en deux modes, soit directement connecté avec le telephone, soit en mode autonome. 
   - Lorsque le telephone se connecte, la carte va d'abord envoyé les données stockée sur la carte SD (depuis l'allumage de la carte), puis envoyé les données en temps réel (à la reception d'une valeur elle etre écrite sur la carte SD puis l'envoyer directement a l'application mobile).
   - Lorsque le telephone n'est pas connecté, à la lecture d'une nouvelle mesure celle-ci est ecrite sur la carte SD (`data`).

### Lecture de l'historique : 

Lorsque le téléphone se connecte, l'historique des valeurs lu depuis sont allumage, concervé sur la carte SD est lu. Sur la carte SD, les valeurs sont placé de la plus ancienne à la valeur la plus recente. Comme nous avons besoin de la valeur la plus recente, la seul mesure ou nous nous connaissons la date de la mesure, date pour laquelle le téléphone c'est connecté, nous devosn envoyé cette valeur en premier.
Ensuite nous depilons la liste, tant que la valeur millis est supếrieur de la mesure courante est inférieur à la la valeur millis de la mesure précedante et nous envoyons cette mesure. 
Pour renversé le fichier, nous utilisons une List, LIFO (last in first out), ainsi la valeur la plus recente ce trouve au sommet du la liste.

NB : du fait que l'on ne puisse pas envoyé beaucoup de bits en même temps, pour une mesure, chaque valeur est envoyées séparemant. Pour que le client puisse reconnaitre la donnée qu'il a recu j'ai indiqué un préfixe permettant de les différanciers.


## Application android : 
Insertion dans la base de données :
  - A l'allumage de l'application, lorsque le téléphone se connecte à la carte Arduino la valeur de epoch() (date courante) est notée dans les sharePréfences, cette donnée va permettre de recalculer la date d'une mesure si la date fourni par le GPS est invalide. 
  - A la reception de chaque composante, il sont placé dans un objet permettant de les saugarder jusqu'a l'envoye de la requete, qui ce passe au moment de la reception de la valeur millis. La seul variable non envoyé, la valeur du millis, préfixé par "m=" sera utilsé pour recalculé, a l'aide de la date courante enregistrer dans le localStorage, la date si celle ci n'est pas correct, ou quel n'est pas vide. Si cette valeur est null cela signifie que nous somme directement connecté avec le téléphone donc nous utiliserons la date courante du téléphone.

----------------

## Suppression des données en dehors de la plage de la semaine ( je ne sais pas encore comment proceder puisque à l'intérieur les valeurs du gps concernant la date/heure sont invalide) 
   Un processus va permettre de supprimer les données du fichier `data` si TS(data) - TS(log) > 7. 
   Pour la suppression on pourrai utilser un fichier temporaire
   - Recopier toutes les données comprises dans cette plage nommée `temp`
   - Supprimer le fichier `data`
   - Renommer le fichier `temp` en `data`

 ## Pour la V2
   Un capteur de CO2 va etre placer sur la carte, on va devoir trouvé une facon de derterminer si le capteur de CO2 est integrer sur la carte.
