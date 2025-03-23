# Character Services Module

This module adds a npc that can be spawned in that you can talk to and purchase a name change, appearance change, race change, or faction change. Every service can be enabled and disabled via the config. You can set specific prices for the services and even enable dynamic cost scaling which adjust the price based on the players level.

## How to install

### 1) Simply place the module under the `modules` folder of your AzerothCore source folder.

### 2) Re-run cmake and launch a clean build of AzerothCore

### 3) Place the character services npc

With a gm account goto the location you want to add the npc and use this command:

```
.npc add 390011
```

### (Optional) Edit module configuration

If you need to change the module configuration, go to your server configuration folder (e.g. **etc**), copy `character_services.conf.dist` to `character_services.conf` and edit it as you prefer.
