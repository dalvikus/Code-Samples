import React from 'react'
import VisibleForm from '../containers/VisibleForm'
import VisibleBookmarkList from '../containers/VisibleBookmarkList'
import FetchEpisodes from '../containers/FetchEpisodes'

const App = () => (
  <div>
    <VisibleForm/>
    <VisibleBookmarkList/>
    <FetchEpisodes/>
  </div>
)

export default App
