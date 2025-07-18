# mod-acore-subscriptions

Handles the subscription logic, no longer require the module(s) or service(s) to have in their code the subscription logic.

Example: [mod-transmog](https://github.com/azerothcore/mod-transmog/commit/8237df6f88d40d1d83a6f11b86a7187f99f57c99).

## Commands

| Command | Description |
|---------|-------------|
| `subscriptions info $name` | Allows you to view a player's current subscription level.<br>**Note:** Players can also use this command, but it will only display their own subscription level. |
| `subscriptions update $name $level` | Allows you to temporarily set a player's subscription level (it will reset after they relog). This is useful for testing. |
