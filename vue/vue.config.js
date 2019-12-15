module.exports = {
  publicPath: process.env.NODE_ENV === 'production'
    ? process.env.ESP_BUILD === 'true' ? '/' : '/AstroRemoteHome/'
    : '/',
  productionSourceMap: false,
  transpileDependencies: ['vuetify'],
  pwa: {
    themeColor: '#1976D2',
    workboxOptions: {
      skipWaiting: true
    }
  },
}
