import React from 'react'

const TextList = ({texts}) => {
  console.log('TextList.render')
    return (
      <ul>
        {texts.map((e) => (<li key={e.id}>|{e.text}|</li>))}
      </ul>
    )
};

export default TextList
