import React from 'react'
import { connect } from 'react-redux'

const TextList = ({texts}) => (
  <ul>
    {texts.map((e) => (<li key={e.id}>{e.text}</li>))}
  </ul>
)

const mapStateToProps = (state) => ({
    texts: state.texts
})

const VisibleTextList = connect(
  mapStateToProps
)(TextList)

export default VisibleTextList
