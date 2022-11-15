import { MemoryRouter as Router, Routes, Route } from 'react-router-dom';
import TopPage from './pages/TopPage';

const App = () => {
  return (
    <Router>
      <Routes>
        <Route path="/" element={<TopPage />} />
      </Routes>
    </Router>
  );
};

export default App;
