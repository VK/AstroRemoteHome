import Vue from 'vue'
import App from './App.vue'
import store from './store'
import VueI18n from 'vue-i18n'

import './registerServiceWorker'
import vuetify from './plugins/vuetify';


var userLangs = navigator.languages;
var selectedLang = "";

userLangs.forEach((l: string) => {

  if (selectedLang === "") {
    if (["de", "en"].includes(l)) {
      selectedLang = l;
      document.documentElement.setAttribute('lang', l);
    }
  }
});


Vue.use(VueI18n);
// setup locale info for root Vue instance
const i18n = new VueI18n({
  locale: selectedLang,
  fallbackLocale: 'en',
  messages: {
    en: {
      word: {
        socket: 'Socket',
        switch: 'Switch',
        mqttConfig: 'MQTT config',
        mqttConnect: 'Connect',
        mqttDisconnect: 'Disconnect',
        time: 'Time',
        close: 'Close',
        save: 'Save',
        delete: 'Delete',
        newSocket: 'New Socket',
        bindOn: 'Learn On',
        bindOff: 'Learn Off',
        deleteSocket: 'Delete Socket',
        deleteSwitch: 'Delete Switch',
        difference: 'Time difference',
        variation: 'Time window',
        irregularity: 'Irregularity',
        irregOn: 'enabled',
        irregOff: 'disabled',
        offtime: 'Minutes off',
        ontime: 'Minutes on',
      },
      explain: {
        learnSwitch: "Please select the switching state you want to learn.\nThen press the remote control until the selected button changes its color.",
        deleteSwitch: "Do you really want to delete the selected automatic switching method of {}?",
        deleteSocket: "Do you really want to delete the socket {}?",
        addSwitchA: "Add automatic switch times with",
        addSwitchB: ".",
        editConfig: 'Enter PIN to edit the configuration.'
      },
      days: {
        mo: "Mo",
        tu: "Tu",
        we: "We",
        th: "Th",
        fr: "Fr",
        sa: "Sa",
        su: "Su"
      },
      swtype: {
        start: "Start",
        end: "End",
        time: "Time",
        sunrise: "Sunrise",
        sunset: "Sunset",
      },
      connState: {
        Connected: "Connected",
        Offline: "Offline",
        Connecting: "Connecting",
        "Connection Error": "Connection Error",
        "Connection Lost": "Connection Lost",
      }
    },
    de: {
      word: {
        socket: 'Steckdose',
        switch: 'Schalter',
        mqttConfig: 'MQTT Konfiguration',
        mqttConnect: 'Verbinden',
        mqttDisconnect: 'Verbindung Trennen',
        time: 'Zeit',
        close: 'Schließen',
        save: 'Speichern',
        delete: 'Löschen',
        newSocket: 'Neue Steckdose',
        bindOn: 'An Lernen',
        bindOff: 'Aus Lernen',
        deleteSocket: 'Steckdose Löschen',
        deleteSwitch: 'Schalter Löschen',
        difference: 'Zeitunterschied',
        variation: 'Zeitfenster',
        irregularity: 'Unregelmäßigkeit',
        irregOn: 'aktiviert',
        irregOff: 'deaktiviert',
        offtime: 'Minuten aus',
        ontime: 'Minuten an',
      },
      explain: {
        learnSwitch: "Bitte wählen Sie den Schaltzustand, den Sie registrieren möchten.\nDrücken Sie dann die Fernbedienung, bis der unten ausgewählte Knopf seine Farbe wechselt.",
        deleteSwitch: "Möchten Sie die automatische Umschaltung von {name} wirklich löschen?",
        deleteSocket: "Möchten Sie die Steckdose {name} wirklich löschen?",
        addSwitchA: "Mit ",
        addSwitchB: " automatische Schaltzeiten hinzufügen.",
        editConfig: 'PIN eingeben, um die Konfiguration zu bearbeiten.'
      },
      days: {
        mo: "Mo",
        tu: "Di",
        we: "Mi",
        th: "Do",
        fr: "Fr",
        sa: "Sa",
        su: "So"
      },
      swtype: {
        start: "Start",
        end: "Ende",
        time: "Uhrzeit",
        sunrise: "Aufgang",
        sunset: "Untergang",
      },
      connState: {
        Connected: "Verbunden",
        Offline: "Offline",
        Connecting: "Verbindunsaufbau",
        "Connection Error": "Verbindungsfehler",
        "Connection Lost": "Verbindung verloren",
      }
    }
  }
})




Vue.config.productionTip = false;

export const bus = new Vue({
  i18n,
  store,
  vuetify,
  render: h => h(App),
  beforeCreate() {
    this.$store.commit("appState/initMQTTconfig");
  }
}).$mount('#app');

