# xway

Projet de commande des trains par Miguel Aderaldo, Julien Gallaud et Guillaume Villeneuve.

## Structure et utilisation

* `gestionnaire.c` contient le code du gestionnaire.
* `train1.c` à `train4.c` contiennent les codes des trains.
* `request.c` contient les codes pour faire et recevoir des requètes Xway.

Pour compiler le projet :

```sh
make
```

Il faut ensuite faire tourner le gestionnaire :
```sh
./gestionnaire
```

Puis faire tourner les trains*i* :
```sh
./train1.c
```

On peut faire tourner les codes sur des machines différentes. 
Il faut seulement veiller à ce que dans le code de chaque train, `gesteIP` corresponde à l'IP de la machine où tourne le gestionnaire.

## Compte rendu
Nous avions réussi à faire tourner les trains indépendament en TP.
Nous avons depuis developpé le gestionnaire de ressources.
Faute de temps, nous n'avons cependant pas put tester l'ensemble.
Les codes des trains communiquent avec le gestionnaire, et ne semblent pas se bloquer mutuellement. La conception semble donc cohérente.
