module.exports = [
  {
    "type": "heading",
    "defaultValue": "Watchface Configuration"
  },
  {
    "type": "text",
    "defaultValue": "This is the settings page for the screen-print watchface."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Colors"
      },
      {
        "type": "color",
        "messageKey": "BackgroundColor",
        "defaultValue": "0x000000",
        "label": "Background Color"
      },
      {
        "type": "color",
        "messageKey": "ForegroundColor",
        "defaultValue": "0xFFFFFF",
        "label": "Foreground Color"
      }
    ]
  },
  {
    "type": "section",
    "capabilities": ["HEALTH"],
    "items": [
      {
        "type": "heading",
        "defaultValue": "Health Settings"
      },
      {
        "type": "toggle",
        "messageKey": "ShakeForSteps",
        "label": "Shake for steps",
        "defaultValue": true
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Weather Settings"
      },
      {
        "type": "toggle",
        "messageKey": "WeatherUseGPS",
        "label": "Use GPS",
        "defaultValue": true
      },
      {
        "type": "input",
        "messageKey": "WeatherLocation",
        "defaultValue": "",
        "label": "Weather Location",
        "attributes": {
          "placeholder": "eg: Berlin",
      }
    }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "More Settings"
      },
      {
        "type": "toggle",
        "messageKey": "VibrateOnDisconnect",
        "label": "Vibrate on disconnect",
        "defaultValue": false
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
